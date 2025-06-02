import { AppSidebar } from "@/components/sidebar/AppSidebar"
import { SidebarProvider, SidebarTrigger } from "@/components/ui/sidebar"
import {
    Breadcrumb,
    BreadcrumbItem,
    BreadcrumbLink,
    BreadcrumbList,
    BreadcrumbPage,
    BreadcrumbSeparator,
} from "@/components/ui/breadcrumb"
import { Separator } from "@/components/ui/separator"
import { Toaster } from "@/components/ui/toaster"
import { MultiTelemetryProvider } from "@/components/context/MultiTelemetryProvider"
import { SelectedDeviceProvider } from "@/components/context/SelectedDeviceProvider"


export default async function Layout({ children }: { children: React.ReactNode }) {

    // const cookieStore = await cookies()
    // const defaultOpen = cookieStore.get("sidebar_state")?.value === "false"

    return (
        <SelectedDeviceProvider>
            <MultiTelemetryProvider>
                <SidebarProvider defaultOpen={false} style={{
                    "--sidebar-width": "20rem",
                    "--sidebar-width-mobile": "20rem",
                }}>
                    <AppSidebar />
                    <main className="flex-grow">
                        <header className="flex h-16 shrink-0 items-center gap-2 transition-[width,height] ease-linear group-has-[[data-collapsible=icon]]/sidebar-wrapper:h-12">
                            <div className="flex items-center gap-2 px-4">
                                <SidebarTrigger className="-ml-1" />
                                <Separator orientation="vertical" className="mr-2 h-4" />
                                <Breadcrumb>
                                    <BreadcrumbList>
                                        <BreadcrumbItem className="hidden md:block">
                                            <BreadcrumbLink href="#">
                                                Building Your Application
                                            </BreadcrumbLink>
                                        </BreadcrumbItem>
                                        <BreadcrumbSeparator className="hidden md:block" />
                                        <BreadcrumbItem>
                                            <BreadcrumbPage>Data Fetching</BreadcrumbPage>
                                        </BreadcrumbItem>
                                    </BreadcrumbList>
                                </Breadcrumb>
                            </div>
                        </header>
                        {/* <MapClientWrapper> */}
                        {children}
                        {/* </MapClientWrapper> */}
                        <Toaster />
                    </main>
                </SidebarProvider >
            </MultiTelemetryProvider>
        </SelectedDeviceProvider>
    )
}
