import { Button } from '../ui/button'
import {
  Dialog,
  DialogClose,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger
} from '../ui/dialog'
import { ProfileForm } from '../forms/ProfileForm'
import { User } from 'lucide-react'
import { Dispatch, SetStateAction } from 'react'

export function ProfileModal({ isOpen, setIsOpen }: { isOpen: boolean, setIsOpen: Dispatch<SetStateAction<boolean>> }) {
  return (
    <Dialog open={isOpen} onOpenChange={(open) => {
      setIsOpen(open)
    }}>
      {/* <DialogTrigger asChild>
      </DialogTrigger> */}
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>Profile</DialogTitle>
          <DialogDescription>
            View your profile information below.
          </DialogDescription>
        </DialogHeader>
        <ProfileForm />
        <DialogFooter>
          <DialogClose asChild>
            <Button type="button" variant="default">
              Close
            </Button>
          </DialogClose>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}
