import { ScrollArea } from "@/components/ui/scroll-area"
import { Dialog, DialogContent, DialogDescription, DialogTitle, DialogTrigger } from "../ui/dialog"
import { Button } from "../ui/button"
import { useEffect, useState } from "react"
import { BadgeAlert } from "lucide-react"
import { getNotifications } from "@/actions"
import NotificationTable from "@/app/notifications/page"


export function NotificationModal() {
    const [isOpen, setIsOpen] = useState<boolean>(false)

    useEffect(() => {
        const loadNotification = async () => {
            const { data } = await getNotifications()
        }

        loadNotification()
    }, [])

    return (
        <Dialog open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DialogTrigger asChild>
                <Button variant="ghost" className="gap-1 w-full justify-between">
                    <p>Notifications Center</p>
                    <BadgeAlert className="h-4 w-4" />
                </Button>
            </DialogTrigger>
            <DialogContent className="sm:max-w-[75vw]">
                <DialogTitle>Notification Center</DialogTitle>
                <DialogDescription>
                    The notification list of all device
                </DialogDescription>
                <ScrollArea className="h-[70vh] w-full rounded-md border p-4">
                    <NotificationTable />
                </ScrollArea>
            </DialogContent>
        </Dialog>
    )
}